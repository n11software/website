sed -i -e 's/3000/3001/g' Website/Source/main.cpp
systemctl stop n11.dev
cmake -GNinja .
ninja
rm -r /web/n11.dev/
mkdir /web/n11.dev/
cp Website/website /web/n11.dev/Link
cp -R www /web/n11.dev/www
cp db.txt /web/n11.dev/db.txt
chown -R web:web /web/n11.dev/
systemctl start n11.dev
