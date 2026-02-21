#!/bin/bash

openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr -subj "/CN=DTLSServer"
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt

openssl genrsa -out client.key 2048
openssl req -new -key client.key -out client.csr -subj "/CN=DTLSClient"
openssl x509 -req -days 365 -in client.csr -signkey client.key -out client.crt
