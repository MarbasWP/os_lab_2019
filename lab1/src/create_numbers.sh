#!/bin/bash

for i in $(seq 1 150); do
  number=$(od -An -N2 -i /dev/urandom | awk '{print $1}')
  echo "$number" >> "numbers.txt"
done