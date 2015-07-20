#ifndef DCM_DCM_H
#define DCM_DCM_H

struct DCMServerPrivate;
struct DCMClientPrivate;
struct DCMSignalPrivate;

typedef struct DCMServer_ {
    struct DCMServerPrivate* dcm;
} DCMServer;

typedef struct DCMClient_ {
    struct DCMClientPrivate* dcm;
} DCMClient;

typedef struct DCMSignal_ {
    struct DCMSignalPrivate *msg;
} DCMSignal;

enum DCMConnectionType{
    TCP,
    UNIX
};

DCMServer *dcm_server_new(DCMConnectionType _type, const char *_endpoint);
void dcm_server_free(DCMServer *_dcm);

DCMClient *dcm_client_new(DCMConnectionType _type, const char *_endpoint);
void dcm_client_free(DCMServer *_dcm);

void dcm_server_on(DCMServer *_dcm, char* _signal, const DCMSignal* _msg);
void dcm_server_off(DCMServer *_dcm, char* _signal);

void dcm_client_send(DCMClient *_scm, const DCMSignal* _msg);

void dcm_signal_set_data();
char* dcm_signal_get_data();
void dcm_signal_erase_data();
void dcm_signal_has_data();

void dcm_signal_set_header();
char* dcm_signal_get_header();
void dcm_signal_erase_header();
void dcm_signal_has_header();

#endif //DCM_DCM_H
