tfvis.visor().surface({ name: "Jugend Forscht", tab: "Input Data" });

// Define your model
const model = tf.sequential();
// input: luz 1, luz 2, luz 3, luz 4, ventana 1, ventana 2, ventana 3, ventana 4, CO2 (500-1500), luminosidad (fuera), movimiento, hora, dia, bluetoothConectado (persona)
model.add(tf.layers.dense({ inputShape: [14], units: 16, activation: "relu" }));
// model.add(tf.layers.dense({ units: 8, activation: "relu" }));
// model.add(tf.layers.dense({ units: 8, activation: "relu" }));
// outputs: luz1,luz2,luz3,luz4, ventana1Motor,
model.add(tf.layers.dense({ units: 5, activation: "softmax" }));

// Compile the model
model.compile({
  optimizer: "sgd",
  loss: "categoricalCrossentropy",
  metrics: ["accuracy"],
});
function getDataSample() {
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
      input: [1, 1, 0.5, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 1],
    },
    {
      input: [1, 1, 0.1, 1, 0, 1, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 1],
    },
    {
      input: [1, 1, 0.9, 1, 0, 0, 0, 1, 0.5, 1, 1, 0.3, 0.3, 1],
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
    {
      input: [0, 1, 0.8, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 0],
    },
    {
      input: [0, 1, 0.9, 1, 0, 0, 1, 0, 0.5, 1, 1, 0.3, 0.3, 1],
      output: [0, 0, 0, 0, 0],
    },
    {
      input: [0, 1, 0.1, 1, 0, 0, 0, 0, 0.5, 1, 1, 0.3, 0.8, 1],
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
    epochs: 200,
    shuffle: true,
    validationData: [validationX, validationY],
    callbacks: fitCallbacks,
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
  // View
  const metrics = ["loss", "val_loss", "acc", "val_acc"];
  const container = {
    name: "Training Progress",
    tab: "Training",
    styles: {
      height: "500px",
    },
  };
  const callbacks = tfvis.show.fitCallbacks(container, metrics);
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
