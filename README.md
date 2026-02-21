Secure Multi-Client Clock Synchronization using Mutual TLS

📌 Project Overview

This project implements a secure clock synchronization system using:

   TCP sockets in C
   OpenSSL TLS 1.3
   Mutual Authentication (mTLS)
   Multi-client handling

The system securely synchronizes client clocks with a centralized time server while ensuring:

   Confidential communication
   Certificate-based authentication
   Protection against MITM attacks

🎯 Objective

To design and implement a secure distributed clock synchronization protocol that:

   1.Uses TLS for encrypted communication
   2.Implements mutual authentication (client + server verification)
   3.Supports multiple simultaneous clients
   4.Measures synchronization offset and network delay
   5.Logs performance metrics

🏗️ System Architecture
        ┌────────────────────┐
        │     TLS Server     │
        │  (Time Authority)  │
        └─────────┬──────────┘
                  │
      ─────────── TLS Secure Channel ───────────
                  │
   ┌──────────────┼──────────────┐
   │              │              │
┌───────┐    ┌───────┐    ┌───────┐
│Client1│    │Client2│    │ClientN│
└───────┘    └───────┘    └───────┘

🔐 Security Features

✅TLS 1.3 Encryption

 All communication is encrypted using OpenSSL.

✅ Mutual TLS Authentication

 -Server verifies client certificate
 -Client verifies server certificate

✅ Certificate-Based Identity

 -Self-signed certificates generated using OpenSSL.

✅ Protection Against:

 -Man-in-the-Middle attacks
 -Replay attacks
 -Eavesdropping

🛠️ Technologies Used

 -C Programming Language
 -POSIX Sockets
 -OpenSSL 3.x
 -WSL Ubuntu
 -GCC Compiler

📂 Project Structure

secure-tls-clock-sync/
│
├── server.c
├── client.c
├── Makefile
├── generate_certs.sh
├── README.md
├── .gitignore

Private keys (*.key) are intentionally excluded for security.

⚙️ Setup Instructions

 1.Install Dependencies (Ubuntu / WSL)
 
   sudo apt update
   sudo apt install build-essential libssl-dev -y
   Verify OpenSSL:
   openssl version

🔑 Generate Certificates

  Run:
  chmod +x generate_certs.sh 
  ./generate_certs.sh
 
  This generates:
  
  1.server.crt
  2.server.key
  3.client.crt
  4.client.key

🧱 Compile Project

  Run:
  make
  To clean:
  make clean

🚀 Running the System

  Start Server
  ./server
  Server waits for client connections.

  Start Client (Same Machine)
  ./client

  Start Client (Another Laptop)

  Change server IP inside client.c:
  #define SERVER_IP "192.168.X.X"
  Recompile:
  make
  Then run client.

⏱️ Clock Synchronization Process

   1.Client records T1 (send time)
   2.Server responds with T2 (server time)
   3.Client records T3 (receive time)
   4.Offset calculated as: Offset = T2 − ((T1 + T3) / 2)
 
   This approximates clock difference accounting for network delay.

📊 Performance Logging
 
   The client measures:
   1.Round-trip delay
   2.Clock offset
   3.Average offset (multiple iterations)

🧪 Multi-Client Handling

   The server uses:
   -fork() for concurrent client handling
   -Independent TLS session per client

   This enables simultaneous synchronization requests.

📈 Results

   Observed:
   
   -Stable offset values
   -Low synchronization delay
   -Successful certificate verification
   -Encrypted session establishment

🔍 Certificate Verification

   Server verifies client: SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL)
   Client verifies server: SSL_CTX_load_verify_locations(...)
 
   Handshake fails if certificate is invalid.

⚠️ Security Best Practices

   -Private keys are excluded from repository
   -TLS 1.3 used
   -Mutual authentication enforced
   -Proper certificate validation enabled

🧩 Challenges Faced

   -DTLS handshake issues
   -WSL networking configuration
   -Certificate validation debugging
   -Multi-client concurrency handling

📌 Future Improvements

   -Drift simulation
   -Network delay simulation
   -CSV logging with graphical analysis
   -NTP protocol comparison
   -Deployment on cloud server

📜 Conclusion

   This project successfully demonstrates secure clock synchronization using:

   -TLS encryption
   -Mutual authentication
   -Multi-client architecture
   -Performance measurement
   
   The system ensures both security and synchronization accuracy in distributed environments.

👩‍💻 Author

Pallavi B
CSE-AIML
