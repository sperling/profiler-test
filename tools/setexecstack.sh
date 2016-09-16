#!/usr/bin/env bash

for i in "$1"/*.so; do execstack -c "$i"; done