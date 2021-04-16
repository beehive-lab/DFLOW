#!/bin/bash

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"

if [ ! -d "$DIR"/venv ]; then
  python3 -m venv "$DIR"/venv &&
    source "$DIR"/venv/bin/activate &&
    pip install wheel &&
    pip install -r "$DIR"/requirements.txt &&
    clear &&
    python3 "$DIR"/main.py
else
  source "$DIR"/venv/bin/activate &&
    pip install -r "$DIR"/requirements.txt &&
    clear &&
    python3 "$DIR"/main.py
fi
