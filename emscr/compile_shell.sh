#!/usr/bin/env bash
hashes=$(git log --pretty=format:"<li>%h - %s<\/li><br\/>" | tr -d '\n')
latest_hash=$(git log --pretty=format:"%h" | head -1)

cat shell.html | sed "s/%VERSIONINFO%/$hashes/g" | sed "s/%LATESTCOMMIT%/$latest_hash/g" > compiled_shell.html
