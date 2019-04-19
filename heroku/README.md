
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
$ git init .
$ git remote -v
$ heroku apps:create {{heroku-app-namne}}
$ git remote rm origin
$ git push heroku master

~~~

