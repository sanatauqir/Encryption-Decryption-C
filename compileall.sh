#!/bin/bash

gcc -g otp_enc.c -o otp_enc
gcc -g otp_enc_d.c -o otp_enc_d
gcc -g otp_dec.c -o otp_dec
gcc -g otp_dec_d.c -o otp_dec_d
gcc -g keygen.c -o keygen
