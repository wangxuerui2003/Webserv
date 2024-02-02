cd www/
sed "s|/webserver-rootpath|$(pwd)|g" ../conf/example.conf > ../conf/local.conf