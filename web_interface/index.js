// _api_ url
var espUrl = "http://varfield.local/";

// Load existing data from localStorage if available
let fetchedDataArray = JSON.parse(localStorage.getItem("fetchedData")) || [];
// Update the table with each item in the fetchedDataArray with a delay
window.addEventListener("load", () => {
  setTimeout(() => {
    fetchedDataArray.forEach((item, index) => {
      setTimeout(
        () => {
          updateTable(item);
        },
        index * (500 / fetchedDataArray.length),
      ); // Adjust the delay (in milliseconds) as needed
    });
  }, 500);
});

tfvis.visor().surface({ name: "Jugend Forscht", tab: "Input Data" });
tfvis.visor().close();
// Define your model
const model = tf.sequential();
// input: luz 1, luz 2, luz 3, luz 4, ventana 1, ventana 2, ventana 3, ventana 4, CO2 (500-1500), luminosidad (fuera), movimiento, hora, dia, bluetoothConectado (persona)
model.add(tf.layers.dense({ inputShape: [14], units: 16, activation: "relu" }));
model.add(tf.layers.dense({ units: 8, activation: "relu" }));
// model.add(tf.layers.dense({ units: 8, activation: "relu" }));
// outputs: luz1,luz2,luz3,luz4, ventana1Motor,
model.add(tf.layers.dense({ units: 5, activation: "softmax" }));

// Compile the model
model.compile({
  optimizer: "adam",
  loss: "categoricalCrossentropy",
  metrics: ["accuracy"],
  earlystop: tf.callbacks.earlyStopping({ monitor: "val_acc" }), //{ monitor: "val_acc" }
});

// function onEpochEnd() {
//   console.log("Epoch End");
// }

function getDataSample() {
  const data = [];

  // Generate 1000 data samples with the first element of the input being 1
  for (let i = 0; i < 1000; i++) {
    const input = [1];
    for (let j = 0; j < 13; j++) {
      input.push(Math.random());
    }
    const output = [0, 0, 0, 0, 1];
    data.push({ input, output });
  }

  // Generate 1000 data samples with the first element of the input being 0
  for (let i = 0; i < 1000; i++) {
    const input = [0];
    for (let j = 0; j < 13; j++) {
      input.push(Math.random());
    }
    const output = [1, 0, 0, 0, 0];
    data.push({ input, output });
  }

  return data;
}

function getDataSampleReal() {
  return [
    {
      input: [1, 1, 0.3, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 1],
    },
    {
      input: [1, 1, 0.2, 1, 0, 1, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 1],
    },
    {
      input: [1, 1, 0.8, 1, 0, 0, 0, 1, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 1],
    },
    {
      input: [0, 1, 0.5, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 0],
    },
    {
      input: [0, 1, 0.2, 1, 0, 0, 1, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 0],
    },
    {
      input: [0, 1, 0.6, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.8, 1],
      output: [0, 0, 0, 0, 0],
    },
  ];

  // return [{1,1,1,1,0,0,0,0,0.5,1,1,0.3,0.3,1}];
  // return Array.from({ length: numSamples }, generateSample);
}
// Generate sample data
const generateSample = () => {
  const input = Array.from({ length: 14 }, () => Math.random());
  const output = Array.from({ length: 5 }, () => Math.random());
  return { input, output };
};

async function train(model, data, fitCallbacks) {
  console.log(data);
  document.querySelector("#styletag").href = "";
  // no slice pq sino no hay datos suficientes todavia
  // const trainingData = data.slice(0, numTrainingSamples);
  const trainingData = data;
  // const validationData = data.slice(numTrainingSamples);
  const validationData = data;

  // Prepare training data
  const trainingX = tf.tensor2d(
    trainingData.map((sample) => sample.input),
    [trainingData.length, trainingData[0].input.length],
  );
  const trainingY = tf.tensor2d(
    trainingData.map((sample) => sample.output),
    [trainingData.length, trainingData[0].output.length],
  );

  // Prepare validation data
  const validationX = tf.tensor2d(
    validationData.map((sample) => sample.input),
    [validationData.length, validationData[0].input.length],
  );
  const validationY = tf.tensor2d(
    validationData.map((sample) => sample.output),
    [validationData.length, validationData[0].output.length],
  );

  // Train the model
  return model.fit(trainingX, trainingY, {
    epochs: 100,
    shuffle: true,
    validationData: [validationX, validationY],
    callbacks: fitCallbacks,
    batchSize: 100,
  });
}
// .then((history) => {
//   console.log("Training complete:", history);

//   // Make predictions on new data
//   const newInput = tf.tensor2d([
//     [
//       /* Your new input data here */
//     ],
//   ]);
//   const prediction = model.predict(newInput);
//   prediction.print();
// })
// .catch((error) => console.error("Error during training:", error));

// Define the watchTraining function

// Generate more sample data
const data = getDataSample();
const numSamples = data.length;
// Split data into training and validation sets
const splitRatio = 0.8;
const numTrainingSamples = Math.floor(numSamples * splitRatio);

async function watchTraining() {
  tfvis.visor().open();

  // View
  const metrics = ["loss", "val_loss", "acc", "val_acc"];
  const container = {
    name: "Training Progress",
    tab: "Training",
    styles: {
      height: "500px",
    },
  };
  // const callbacks = tfvis.show.fitCallbacks(container, metrics);
  const callbacks = [
    tfvis.show.fitCallbacks(container, metrics),
    // tf.callbacks.earlyStopping(), //{ monitor: "val_acc" }
    // new tf.CustomCallback({
    //   onEpochEnd: onEpochEnd(),
    // }),
  ];

  return train(model, data, callbacks);
  // // Train the model with callbacks for visualization
  // await model.fit(trainingX, trainingY, {
  //   epochs: 50,
  //   shuffle: true,
  //   validationData: [validationX, validationY],
  //   callbacks,
  // });

  // // Make predictions on new data
  // const newInput = tf.tensor2d([[0.5, 1, 0, 0, 0, 1, 0, 1, 0.8, 1]]);
  // const prediction = model.predict(newInput);
  // prediction.print();
}

// Add event listener to start training
document
  .querySelector("#start-training")
  .addEventListener("click", () => watchTraining());
document.querySelector("#show-visor").addEventListener("click", () => {
  tfvis.visor().open();
});
function predictionOd() {
  // Given input data for prediction
  const inputData = tf.tensor2d([
    [1, 1, 1, 1, 0, 0, 0, 1, 0.5, 1, 1, 0.3, 0.3, 1],
  ]);

  // Use the trained model for prediction
  const prediction = model.predict(inputData);

  // Print the predicted output
  prediction.print();
}

// get and store data
function fetchData() {
  fetch(espUrl + "get")
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((data) => {
      fetchedDataArray.push(data);
      // Save the updated array in localStorage
      localStorage.setItem("fetchedData", JSON.stringify(fetchedDataArray));
      // Update the table with the fetched data
      updateTable(data);
    })
    .catch((error) => {
      console.error("Error fetching data:", error);
    });
}

function updateTable(data) {
  const tableBody = document
    .getElementById("data-table")
    .getElementsByTagName("tbody")[0];
  const newRow = tableBody.insertRow(0);

  Object.values(data).forEach((value) => {
    const cell = newRow.insertCell();
    if (Array.isArray(value)) {
      cell.textContent = `[${value.join(", ")}]`;
    } else {
      cell.textContent = value;
    }
  });

  // Highlight the newly added row
  newRow.classList.add("highlight");

  // Remove the highlight class after the transition ends
  setTimeout(() => {
    newRow.classList.remove("highlight");
  }, 500);
}

// Set up a periodic fetch every 5 seconds (adjust the interval as needed)
const fetchInterval = setInterval(fetchData, 5000);

// To stop the periodic fetch, you can use clearInterval(fetchInterval);

// remote control
document.getElementById("sendControlCmd").addEventListener("click", () => {
  let theInput = document.getElementById("controlInput");
  let cmd = theInput.value;
  sendCommand(cmd);
  theInput.value = "";
});
function sendCommand(cmd) {
  fetch(espUrl + "send?cmd=" + encodeURIComponent(cmd))
    .then((response) => {
      return response.text();
    })
    .then((text) => {
      if (text == "OK") {
        // all ok
      } else {
        alert("error on send");
      }
    });
}

document.getElementById("lightsON").addEventListener("click", () => {
  sendCommand("sl0l01l11l21l31");
});
document.getElementById("lightsOFF").addEventListener("click", () => {
  sendCommand("sl0l00l10l20l30");
});
document.getElementById("lightThresholdBtn").addEventListener("click", () => {
  let theInput = document.getElementById("lightThreshold");
  let cmd = theInput.value;
  if (cmd > 9) {
    theInput.value = 9;
  } else if (cmd < 0) {
    theInput.value = 0;
  }
  cmd = theInput.value;
  sendCommand("lt" + cmd);
  theInput.value = "";
});
setInterval(() => {
  if (
    document.querySelector("#styletag").href != "https://localhost/style.css"
  ) {
    document.querySelector("#styletag").href = "/style.css";
  }
}, 120000);
document
  .getElementById("predictionValueRequest")
  .addEventListener("keyup", () => {
    loadInputColorbar();
  });
function loadInputColorbar() {
  const colorBar = document.getElementById("colorBarInput");

  // Set the background gradient based on the valuesArray
  const gradient = JSON.parse(
    document.getElementById("predictionValueRequest").value,
  ).map((value) => {
    const color = `rgba(0, 0, 255, ${1 - value})`; // Dark blue to yellow gradient
    return color;
  });

  colorBar.style.background = `linear-gradient(to right, ${gradient.join(
    ", ",
  )})`;
}
loadInputColorbar();
let exampleAnimateInterval = null;
function animateInputExamples(type = null) {
  if (exampleAnimateInterval) {
    clearInterval(exampleAnimateInterval);
    exampleAnimateInterval = null;
  }
  if (!type) {
    document.getElementById("colorBar").style = "background: black";
    return;
  }
  let input = [1];
  exampleAnimateInterval = setInterval(() => {
    if (type > 0.5) {
      document.getElementById("colorBar").style =
        "background: linear-gradient(to right, rgb(0, 0, 255), rgb(0, 0, 255), rgb(0, 0, 255), rgb(0, 0, 255), rgba(0, 0, 255, 0));";
      input = [1];
      for (let j = 0; j < 13; j++) {
        input.push(Math.random());
      }
    } else {
      document.getElementById("colorBar").style =
        "background: linear-gradient(to right, rgb(0, 0, 255, 0), rgb(0, 0, 255), rgb(0, 0, 255), rgb(0, 0, 255), rgba(0, 0, 255));";
      input = [0];
      for (let j = 0; j < 13; j++) {
        input.push(Math.random());
      }
    }
    document.getElementById("predictionValueRequest").value =
      JSON.stringify(input);
    loadInputColorbar();
  }, 25);
}
document.getElementById("predictNow").addEventListener("click", () => {
  let theInput = document.getElementById("predictionValueRequest");
  // console.log(JSON.parse(theInput.value));
  let ts2d = tf.tensor2d(JSON.parse(theInput.value), [1, 14]);
  let predictionArray = Array.from(model.predict(ts2d).dataSync());
  document.getElementById("predictOutput").textContent =
    JSON.stringify(predictionArray);
  // Get the color bar element
  const colorBar = document.getElementById("colorBar");

  // Set the background gradient based on the valuesArray
  const gradient = predictionArray.map((value) => {
    const color = `rgba(0, 0, 255, ${1 - value})`; // Dark blue to yellow gradient
    return color;
  });

  colorBar.style.background = `linear-gradient(to right, ${gradient.join(
    ", ",
  )})`;
});
