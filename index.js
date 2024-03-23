const { spawn } = require('child_process');
const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const cors = require('cors');

const app = express();
const port = 3000; 
app.use(cors())
// bodyParser middleware to parse JSON requests
app.use(bodyParser.json());

// Path to the C++ program
const cppProgramPath = path.resolve(__dirname, 'harmonia_sequential_inserts');

// spawning the C++ program as a child process
app.post('/insert', (req, res) => {
  const { key, value } = req.body;
  const inputData = ['1','1' , key , ...value];
  let retVal ;
  if (key && value) {

    const cppProcess = spawn(cppProgramPath, inputData);

    // handling the standard output data
    cppProcess.stdout.on('data', (data) => {
      retVal = data.toString()
      // console.log(`Output from C++ program: ${data.toString()}`);
    });
    
    // handling errors
    cppProcess.stderr.on('data', (data) => {
      console.error(`Error from C++ program: ${data.toString()}`);
    });
    
    // handling the end of the process
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
  const lines = inputString.split('\n');
  let n = lines.length
  
  const numericValues = lines.slice(1, n-2).map(value => value.trim());

  const timeTakenValue = lines[n-2].split(': ')[1].trim();

  return { value: numericValues, timeTakenInMicroSeconds: timeTakenValue };
}

app.post('/search', (req, res) => {
  const {key} = req.body;
  
  if (key ) {
    const inputData = ['1','2' , key ];
    const cppProcess = spawn(cppProgramPath, inputData);
    let retVal ;
    // handling the standard output data
    cppProcess.stdout.on('data', (data) => {
      // console.log(`Output from C++ program: ${data.toString()}`);
      retVal = data.toString()
      // console.log(retVal)
    });
    
    // handling errors
    cppProcess.stderr.on('data', (data) => {
      console.error(`Error from C++ program: ${data.toString()}`);
    });
    
    // handling the end of the process
    cppProcess.on('close', (code) => {
      console.log(`C++ program exited with code ${code}`);
      let success = true
      if(retVal.length < 102) {
       success = false
      }
      const {value , timeTakenInMicroSeconds} = parseString(retVal)
      res.json({ success , data: {value , timeTakenInMicroSeconds} });
    });
  
  } else {
      res.status(404).json({ success: false, message: 'Key not found' });
  }
});

// Start the server
app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});

