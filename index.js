const { spawn } = require('child_process');
const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const cors = require('cors');

const app = express();
const port = 3000; // You can use any port you prefer
app.use(cors())
// Use bodyParser middleware to parse JSON requests
app.use(bodyParser.json());

// Path to your C++ program
const cppProgramPath = path.resolve(__dirname, 'harmonia_sequential_inserts');

// Spawn the C++ program as a child process
app.post('/insert', (req, res) => {
  const { key, value } = req.body;
  const inputData = ['1','1' , key , ...value];
  let retVal ;
  if (key && value) {

    const cppProcess = spawn(cppProgramPath, inputData);

    // Handle the standard output data
    cppProcess.stdout.on('data', (data) => {
      retVal = data.toString()
      console.log(`Output from C++ program: ${data.toString()}`);
    });
    
    // Handle errors
    cppProcess.stderr.on('data', (data) => {
      console.error(`Error from C++ program: ${data.toString()}`);
    });
    
    // Handle the end of the process
    cppProcess.on('close', (code) => {
      console.log(`C++ program exited with code ${code}`);
      retVal = retVal?.trim();
      const match = retVal.match(/Time taken: \d+/);
      let timeTaken;
      if (match) {
       timeTaken = match[0].split(":")[1].trim();
      }
      const data = { message: 'Data inserted successfully', timeTakenInMicroSeconds : timeTaken };
      res.json({ success: true, data: data });

    });

  } else {
      res.status(400).json({ success: false, message: 'Both key and value are required' });
  }
});

function parseString(inputString) {
  // Split the input string by newline character
  const lines = inputString.split('\n');

  // Extract numeric values from the second line to the last but one and remove carriage return characters
  const numericValues = lines.slice(1, 51).map(value => value.trim());

  // Extract the value after "Time taken:" and remove carriage return characters
  const timeTakenValue = lines[51].split(': ')[1].trim();

  // Return an array containing the numeric values as strings and the time taken value
  return { value: numericValues, timeTakenInMicroSeconds: timeTakenValue };
}

app.post('/search', (req, res) => {
  const {key} = req.body;
  
  if (key ) {
    const inputData = ['1','2' , key ];
    const cppProcess = spawn(cppProgramPath, inputData);
    let retVal ;
    // Handle the standard output data
    cppProcess.stdout.on('data', (data) => {
      console.log(`Output from C++ program: ${data.toString()}`);
      retVal = data.toString()
      console.log(retVal)
    });
    
    // Handle errors
    cppProcess.stderr.on('data', (data) => {
      console.error(`Error from C++ program: ${data.toString()}`);
    });
    
    // Handle the end of the process
    cppProcess.on('close', (code) => {
      console.log(`C++ program exited with code ${code}`);
      const {value , timeTakenInMicroSeconds} = parseString(retVal)
      res.json({ success: true, data: {value , timeTakenInMicroSeconds} });
    });
  
  } else {
      res.status(404).json({ success: false, message: 'Key not found' });
  }
});

// Start the server
app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});

