echo "proxy server running on port"
echo $1
make clean
make all
./bin/runserver $1