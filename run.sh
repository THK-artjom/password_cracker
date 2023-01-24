#!/bin/bash

mpiexec -np 4 Program.exe -pw "password" -characterSet "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" -logLevel "Info" -maxPasswordLength 12 > Program.log