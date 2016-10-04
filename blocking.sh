#!/bin/bash

N=0
PORT=1234
while true
do
  echo "Listening on $PORT..."
  nc -l $PORT
  ((PORT++))

  ((N++))
  echo "Received connection $N"
done
