#!/bin/bash

#./get-map-list.py
curl -s https://tempus.xyz/api/maps/list | grep -o "\"name\": \"[a-zA-Z0-9_]*\"" | cut -d' ' -f 2 | cut -c 2- | sed 's/.$//' > maps.csv

echo maps.csv file created
