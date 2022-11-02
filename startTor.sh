#!/bin/bash

# echo "changeme" | sudo -S service tor stop
echo "changeme" | sudo -S service tor reload
echo "changeme" | sudo -S service tor restart
# echo "changeme" | sudo -S service tor status

tor-ctrl signal NEWNYM