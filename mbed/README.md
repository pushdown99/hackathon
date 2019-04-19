


~~~bash
$ mbed import http://os.mbed.com/teams/ST/code/pelion-example-disco-iot01/
$ cd pelion-example-disco-iot01
$ mbed config -G CLOUD_SDK_API_KEY <PELION_DM_API_KEY>
$ mbed dm init -d "https://api.us-east-1.mbedcloud.com" --model-name "DISCO_L475VG_IOT01A" -q --force
$ mbed add http://os.mbed.com/teams/sandbox/code/mbed-http/
$ mbed compile -t GCC_ARM -m DISCO_L475VG_IOT01A
~~~

