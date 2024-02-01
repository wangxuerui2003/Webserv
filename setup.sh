cp conf/example.conf conf/local.conf

cd www/

sed -i "s#/webserver-rootpath#$(pwd)#g" ../conf/local.conf