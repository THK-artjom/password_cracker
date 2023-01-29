#!/bin/bash

mpiexec -np 4 Program.exe -pw "passw" -characterSet "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" -logLevel "Info" -maxPasswordLength 7 > Program.log & tail -f ./Program.log
