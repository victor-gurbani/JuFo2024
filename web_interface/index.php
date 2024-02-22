<html>
  <head>
    <!-- <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs@latest"> </script> -->
    <script src="tfjs.js"> </script>
    <!-- <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs-vis@latest"> </script> -->
    <script src="tfjs-vis.js"> </script>
    <meta charset="utf-8">
    <meta name="theme-color" content="#007bff">
    <link rel="apple-touch-icon" href="/V-Logo-Maskable.png">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link id="styletag" rel="stylesheet" href="/style.css">
    <!-- <link rel="canonical" href="https://varfield.sytes.net/"/> -->
    <link rel="icon" type="image/x-icon" href="/V-Logo.png">
    <title>Jugend Forscht</title>
    <meta property="og:title" content="Jugend Forscht" />
    <!-- <meta property="og:url" content="https://varfield.sytes.net/" /> -->
    <!-- <meta name="description" content=' testing'> -->
    <!-- <meta property="og:description" content=' testing'> -->
    <style>
      div.table {
/*        max-width: 80%;*/
        width: 100%;
        height: 80vh;
        overflow-x: auto;

  overflow: auto;


  background:
    /* Shadow covers */
    linear-gradient(white 30%, rgba(255,255,255,0)),
    linear-gradient(rgba(255,255,255,0), white 70%) 0 100%,
    
    /* Shadows */
    radial-gradient(50% 0, farthest-side, rgba(0,0,0,.2), rgba(0,0,0,0)),
    radial-gradient(50% 100%,farthest-side, rgba(0,0,0,.2), rgba(0,0,0,0)) 0 100%;
  background:
    /* Shadow covers */
    linear-gradient(white 30%, rgba(255,255,255,0)),
    linear-gradient(rgba(255,255,255,0), white 70%) 0 100%,
    
    /* Shadows */
    radial-gradient(farthest-side at 50% 0, rgba(0,0,0,.2), rgba(0,0,0,0)),
    radial-gradient(farthest-side at 50% 100%, rgba(0,0,0,.2), rgba(0,0,0,0)) 0 100%;
  background-repeat: no-repeat;
  background-color: white;
  background-size: 100% 40px, 100% 40px, 100% 14px, 100% 14px;
  
  /* Opera doesn’t support this in the shorthand */
  background-attachment: local, local, scroll, scroll;
      }
      table {
        border-collapse: collapse;
        width: 100%;
        height: 100%;
/*        overflow-x: auto;*/
      }
      th, td {
        border: 1px solid #ddd;
        padding: 8px;
        text-align: left;
        transition: all 0.5s ease;
      }
      tr {
        transition: all 0.5s ease;
      }
      th {
        background-color: #f2f2f2;
      }
      .highlight {
        background-color: yellow;
        scale: 1.01;
      }

      #colorBar, #colorBarInput {
/*            width: 300px;*/
            height: 20px;
            background: linear-gradient(to right, yellow, yellow);
        }
    </style>
  </head>
  <body>
    <header>
      <h1>Victor Gurbani Jugend Forscht | Daten und Künstliche Intelligenz </h1> 
      <img src="https://www.jugend-forscht.de/fileadmin/assets/img/logo_jugend-forscht_wir-foerdern-talente.svg" style="
        height: 2em;
/*        float: left;*/
        background-color: white;
        padding: 0.3em;
        margin: 0.2em;
    ">
    </header>
    <main style="width: calc(100% - 40px); max-width: 100%;height: calc(100vh - 120px);">
      <!-- <div id="micro-out-div">Training...</div> -->
      <h2>Ferngesteuertes Haus Kontrolle</h2>
      <br/>
      <details>
        <summary>Other commands</summary>
        <fieldset>
          <!-- <legend>Other commands</legend> -->
          <button id="lightsON">Turn All Lights ON</button><button id="lightsOFF">Turn All Lights OFF</button>
          <hr/>
          <label for="lightThreshold">Light threshold: </label><input min="1" max="9" type="number" id="lightThreshold"><button id="lightThresholdBtn">Set!</button>
        </fieldset>
        <br/>
      </details>
      <br/>
      <input type="text" id="controlInput"> <button id="sendControlCmd">Send</button>
      <hr>
      <h2>Training Kontrolle</h2><br>
      <button id="start-training">start</button>
      <button id="show-visor">Show</button>
      <fieldset>
        <legend> Model Predictions </legend>
        <details>
          <summary>I/O info</summary>
          <p style="font-size: 0.6em;"><i>input:</i> luz 1, luz 2, luz 3, luz 4, ventana 1, ventana 2, ventana 3, ventana 4, CO2 (500-1500), luminosidad (fuera), movimiento, hora, dia, bluetoothConectado (persona)</p>
          <p style="font-size: 0.6em;"><i>output: </i> luz1,luz2,luz3,luz4, ventana1Motor</p>
        </details>
        <input type="text" placeholder="2dtensor of values [ , , , , ]" value="[1, 1, 0.3, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1]" id="predictionValueRequest">
        <div style="background: yellow;" ><div id="colorBarInput"></div></div><br/>
        <!-- <details>
          <summary>exampleanimation</summary>
          <p style="font-size: 0.6em;"><i>input:</i> luz 1, luz 2, luz 3, luz 4, ventana 1, ventana 2, ventana 3, ventana 4, CO2 (500-1500), luminosidad (fuera), movimiento, hora, dia, bluetoothConectado (persona)</p>
          <p style="font-size: 0.6em;"><i>output: </i> luz1,luz2,luz3,luz4, ventana1Motor</p>
        </details> -->
        <button id="predictNow">Predict</button><br/>
        <pre id="predictOutput"></pre>
        <br/>
            
        <div style="background: yellow;" ><div id="colorBar"></div></div>

      
      </fieldset>
      <h3>Daten</h3>
      <div class="table" style="max-height:100dvh;">
        <table id="data-table">
          <thead>
            <tr>
              <th>ppm_a</th>
              <th>ppm_u</th>
              <th>ppm_p</th>
              <th>windows</th>
              <th>threshold</th>
              <th>servoangle</th>
              <th>motorAllowed</th>
              <th>millis</th>
              <th>lights</th>
              <th>lightSensors</th>
              <th>lightAllowed</th>
              <th>lastMotionDelta</th>
              <th>message</th>
            </tr>
          </thead>
          <tbody>
            <!-- Table rows will be added dynamically here -->
          </tbody>
        </table>
      </div>
      <script src="./index.js"> </script>
    </main>
    <div class="background-shapes">
      <div class="background-shapes__circle"></div>
      <div class="background-shapes__rect"></div>
      <div class="background-shapes__rect"></div>
      <div class="background-shapes__rect"></div>
      <div class="background-shapes__rect"></div>
    </div>
  </body>
</html>