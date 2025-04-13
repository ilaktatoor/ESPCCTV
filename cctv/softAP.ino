void handlePortal() {
  if (server.method() == HTTP_POST) {
    strncpy(user_wifi.ssid,        server.arg("ssid").c_str(),        sizeof(user_wifi.ssid));
    strncpy(user_wifi.password,    server.arg("password").c_str(),    sizeof(user_wifi.password));
    strncpy(user_wifi.camid,       server.arg("camid").c_str(),       sizeof(user_wifi.camid));
    strncpy(user_wifi.server_host, server.arg("server_host").c_str(), sizeof(user_wifi.server_host));
    user_wifi.port = server.arg("port").toInt();

    user_wifi.ssid[sizeof(user_wifi.ssid) - 1] = '\0';
    user_wifi.password[sizeof(user_wifi.password) - 1] = '\0';
    user_wifi.camid[sizeof(user_wifi.camid) - 1] = '\0';
    user_wifi.server_host[sizeof(user_wifi.server_host) - 1] = '\0';

    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please restart the device.</p></main></body></html>" );
  } else {
    server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center}</style></head><body><main class='form-signin'><form action='/' method='post'><h1 class=''>Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'></div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><div class='form-floating'><br/><label>Camera ID</label><input type='text' class='form-control' name='camid'></div><div class='form-floating'><br/><label>Server IP/Domain</label><input type='text' class='form-control' name='server_host'></div><div class='form-floating'><br/><label>Port</label><input type='number' class='form-control' name='port'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'><a href='http://www.stdevsec.com' style='color: #32C5FF'>stdevsec.com</a></p></form></main></body></html>");
  }
}
