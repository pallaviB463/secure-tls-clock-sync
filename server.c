#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <time.h>

#define PORT 4444
#define BUFFER_SIZE 1024

SSL_CTX *ctx;

double current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_sock);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(client_sock);
        return NULL;
    }
    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        printf("Client certificate verification failed\n");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_sock);
        return NULL;
    }

    printf("Client certificate verified successfully.\n");

    printf("Client connected securely.\n");

    char buffer[BUFFER_SIZE];

    while (1) {


       int n = SSL_read(ssl, buffer, BUFFER_SIZE);

       if (n <= 0)
          break;

       buffer[n] = '\0';

       double T1;
       sscanf(buffer, "%lf", &T1);

       double T2 = current_time();
       double T3 = current_time();

       sprintf(buffer, "%lf %lf %lf", T1, T2, T3);
       SSL_write(ssl, buffer, strlen(buffer));
    }


    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_sock);

    printf("Client disconnected.\n");
    return NULL;
}

int main() {

    SSL_library_init();
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_server_method());
    // Load trusted client certificate
    if (!SSL_CTX_load_verify_locations(ctx, "client.crt", NULL)) {
      printf("Error loading client certificate\n");
      exit(1);
    }

    // Require client certificate
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 1);
    SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 10);

    printf("TLS Clock Server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &len);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    close(server_sock);
    SSL_CTX_free(ctx);
    return 0;
}
