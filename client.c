#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 4444
#define BUFFER_SIZE 1024

double current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {

    SSL_library_init();
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    // Load client certificate
    if (SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM) <= 0) {
       printf("Error loading client certificate\n");
       exit(1);
    }

    // Load client private key
    if (SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM) <= 0) {
       printf("Error loading client private key\n");
       exit(1);
    }

    // Load trusted server certificate
    if (!SSL_CTX_load_verify_locations(ctx, "server.crt", NULL)) {
       printf("Error loading server certificate\n");
       exit(1);
    }

    // Require certificate verification
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    // Verify certificate result
    if (SSL_get_verify_result(ssl) != X509_V_OK) {
       printf("Certificate verification failed\n");
       SSL_free(ssl);
       close(sock);
       SSL_CTX_free(ctx);
       return 1;
    }

    printf("Certificate verified successfully.\n");

    printf("Secure connection established.\n");
    // ===== PERFORMANCE LOGGING SETUP =====
    FILE *log = fopen("results.csv", "w");
    fprintf(log, "Cycle,Delay,Offset\n");

    double total_delay = 0;
    double total_offset = 0;
    // ======================================
    for(int i = 0; i < 5; i++) {

       double T1 = current_time();
       char buffer[BUFFER_SIZE];

       sprintf(buffer, "%lf", T1);
       SSL_write(ssl, buffer, strlen(buffer));

       int n = SSL_read(ssl, buffer, BUFFER_SIZE);
       double T4 = current_time();

       if (n > 0) {
           buffer[n] = '\0';

           double rT1, T2, T3;
           sscanf(buffer, "%lf %lf %lf", &rT1, &T2, &T3);

           double delay = (T4 - rT1) - (T3 - T2);
           double offset = ((T2 - rT1) + (T3 - T4)) / 2;
           // ===== LOGGING + ACCUMULATION =====
           fprintf(log, "%d,%lf,%lf\n", i+1, delay, offset);

           total_delay += delay;
           total_offset += offset;
           // ==================================
           printf("Cycle %d\n", i+1);
           printf("Delay: %lf\n", delay);
           printf("Offset: %lf\n\n", offset);
        }

        sleep(2);  // wait 2 seconds before next sync
    }
    // ===== CLOSE FILE + PRINT AVERAGE =====
    fclose(log);

    printf("Average Delay: %lf\n", total_delay / 5);
    printf("Average Offset: %lf\n", total_offset / 5);
    // ======================================

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    return 0;
}
