#ifndef HTML_H
#define HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     font-size: 3.0rem;
     background-color: black;
     color: white;
    }
  </style>
</head>
<body>
  <h3>Twinkle Controller</h3>
  <div>Reset reason: %RESET%</div>
  <iframe name="dummyframe" id="dummyframe" style="display: none;"></iframe>
  <form action="/bright" method="post" target="dummyframe">
    <button type="submit">Bright</button>
  </form>
  <form action="/off" method="post" target="dummyframe">
    <button type="submit">Off</button>
  </form>
  <form action="/twinkle" method="post" target="dummyframe">
    <button type="submit">Twinkle</button>
  </form>
</body>
</html>)rawliteral";

#endif

