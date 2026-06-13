#!/bin/bash

COMMANDE="sudo ./ft_ping google.com"

make fclean
make
make clean

if [ $? -ne 0]; then
    echo "compilation fail check u code :/"
    exit 1
fi

echo "compilation ok ;D"
echo ""
echo "........let's go........."

echo "..........start ping google.com........"

gnome-terminal -- bash -c "$COMMANDE; exec bash"