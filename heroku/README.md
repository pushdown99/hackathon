
https://devcenter.heroku.com/articles/heroku-cli

~~~bash

$ curl https://cli-assets.heroku.com/install-ubuntu.sh | sh
$ heroku --version
heroku/7.0.0 (darwin-x64) node-v8.0.0

$ heroku login
heroku: Enter your login credentials
Email: me@example.com
Password: 
Two-factor code: 
Logged in as me@heroku.com

$ git clone https://github.com/pushdown99/mbed-iot.git
$ cd mbed-iot
$ git remote -v
$ git remote rm origin
$ git init .
$ heroku apps:create {{heroku-app-namne}}
$ heroku buildpacks:set heroku/php
$ heroku buildpacks:add heroku/python
$ heroku addons:create heroku-postgresql:hobby-dev
$ git push heroku master


## copy and paster pg/table.sql
$ heroku pg:psql
--> Connecting to postgresql
psql (9.5.14, server 11.2 (Ubuntu 11.2-1.pgdg16.04+1))
WARNING: psql major version 9.5, server major version 11.
         Some psql features might not work.
SSL connection (protocol: TLSv1.2, cipher: ECDHE-RSA-AES256-GCM-SHA384, bits: 256, compression: off)
Type "help" for help.

DATABASE=> CREATE...

$ heroku ps:scale web=1
$ heroku open

~~~

