#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>

#include <err.h>
#include <assert.h>

#include <zmq.h>
#include <event.h>
#include <evhttp.h>

struct instance {
    const char *output_host;
    unsigned short output_port;
    const char *output_path;

    struct evhttp_connection *output_con;

    struct event_base *ev_base;
    struct event ev_input;

    void *zmq_ctx;
    void *zmq_pull;

    unsigned int stats_open;
    int stopping;
};
typedef struct instance * instance_t;

instance_t g_instance = NULL;

/** Trap Ctrl-C and safely stop */
static void
on_signal(int sig) {
    if(g_instance && sig == SIGINT) {
        g_instance->stopping = 1;
        event_base_loopexit(g_instance->ev_base, NULL);
    }
}

/** When request is finished - update stats */
static void output_finished_cb(struct evhttp_request *_req, void *_self) {
    instance_t self = (instance_t)_self;
    (void)_req;
    self->stats_open--;
}

/** When HTTP connection closed, cleanup */
static void output_closed_cb(struct evhttp_connection *evcon, void *_self) {
    instance_t self = (instance_t)_self;
    assert( evcon == self->output_con );
}

/** Send ZeroMQ message to output over HTTP */
static int output_send(instance_t self, zmq_msg_t *msg) {
    struct evhttp_request *req;

    if(!self->output_con) {
        self->output_con = evhttp_connection_new(self->output_host, self->output_port);
        assert( self->output_con != NULL );
        evhttp_connection_set_base(self->output_con, self->ev_base);
        evhttp_connection_set_closecb(self->output_con, output_closed_cb, self);
    }

    evhttp_connection_set_timeout(self->output_con, 120);
    evhttp_connection_set_retries(self->output_con, 2);

    req = evhttp_request_new(output_finished_cb, self);
    assert( req != NULL );
    evhttp_add_header(req->output_headers, "Host", self->output_host);
    evhttp_add_header(req->output_headers, "Connection", "Keep-Alive");
    evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=UTF-8");
    evbuffer_add(req->output_buffer, zmq_msg_data(msg), zmq_msg_size(msg));

    self->stats_open++;

    return evhttp_make_request(self->output_con, req, EVHTTP_REQ_POST, self->output_path);
}

/** Receive ZeroMQ message */
static int input_recv(instance_t self) {
    int got_msg;
    zmq_msg_t msg;

    zmq_msg_init(&msg);
    got_msg = zmq_recv(self->zmq_pull, &msg, ZMQ_NOBLOCK) != -1;
    if(got_msg) {
        output_send(self, &msg);
    }
    zmq_msg_close(&msg);

    return got_msg;
}

/** Process triggered input events */
static void
input_cb(int _, short __, void *_self) {
    instance_t self = (instance_t)_self;
    uint32_t events = 0;
    size_t len = sizeof(events);

    (void)_; /* <-- 凸(⋋▂⋌)凸 --> */ (void)__;

    if(zmq_getsockopt(self->zmq_pull, ZMQ_EVENTS, &events, &len) == -1) {
        err(EXIT_FAILURE, "zmq_getsockopt(ZMQ_EVENTS)");
    }

    if( events & ZMQ_POLLIN ) {
        while( input_recv(self) ) {
            /* nothing to see here, move along */
        }
    }
}

static void init_input(instance_t self, char *bind_addr) {
    int fd;
    size_t len;

    /* Bind socket */
    self->zmq_pull = zmq_socket(self->zmq_ctx, ZMQ_PULL);
    assert( self->zmq_pull != NULL );
    if(zmq_bind(self->zmq_pull, bind_addr) != 0) {
        err(EXIT_FAILURE, "zmq_bind(%s)", bind_addr);
    }

    /* Add ZMQ pull to event dispatcher */
    len = sizeof(self->zmq_pull);
    if(zmq_getsockopt(self->zmq_pull, ZMQ_FD, &fd, &len) == -1) {
        err(EXIT_FAILURE, "zmq_getsockopt(ZMQ_FD)");
    }
    event_set(&self->ev_input, fd, EV_READ | EV_PERSIST, input_cb, self);
    event_base_set(self->ev_base, &self->ev_input);

    if(event_add(&self->ev_input, NULL) != 0 ) {
        errx(EXIT_FAILURE, "Could not event_add() for ZMQ input");
    }
}

static void
init_output(instance_t self, const char *host, 
            const char *port_str, const char *path) {
    int port;

    port = atoi(port_str);
    if( port < 1 || port > 0xffff ) {
        warnx("init_self(port='%s') - invalid port, using default '80'", port_str);
        port = 80;
    }

    self->output_host = host;
    self->output_port = port;
    self->output_path = path;
}

static void init_self(instance_t self) {
    memset(self, 0, sizeof(struct instance));

    self->zmq_ctx = zmq_init(1);
    self->ev_base = event_base_new();
}

static int
run(instance_t self) {
    event_base_dispatch(self->ev_base);
    if(self->output_con) {
        evhttp_connection_free(self->output_con);
    }
    event_base_free(self->ev_base);
    zmq_close(self->zmq_pull);
    zmq_term(self->zmq_ctx);
    return EXIT_SUCCESS;
}

int
main(int argc, char **argv) {
    struct instance self;

    if( argc < 4 ) {
        errx(EXIT_FAILURE, "Usage: %s <input-zmq-socket> <output-http-host> <output-http-port> <output-http-path>\n", basename(argv[0]));
    }

    init_self(&self);
    init_output(&self, argv[2], argv[3], argv[4]);
    init_input(&self, argv[1]);

    signal(SIGINT, on_signal);
    g_instance = &self;

    return run(&self);
}
