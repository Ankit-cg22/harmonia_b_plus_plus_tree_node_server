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

// Data to send to the C++ program
// const inputData = '1 1 k v';
// const inputData = ['1','1' , '22' , '33']; // for running 1 command and in that command we insert a k,v pair
// const inputData = ['1','2' , '25'];
// 2 queries:
// 1 , 23 , 24 : of type 1 , means to insert 23,24 pair
// 2 , 23 :  search for key 23

// Spawn the C++ program as a child process
app.post('/insert', (req, res) => {
  const { key, value } = req.body;
  const inputData = ['1','1' , key , value];
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

app.post('/search', (req, res) => {
  const {key} = req.body;
  const inputData = ['1','1' ,key];
  
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
      retVal = retVal?.trim();
      const match = retVal.match(/(-?\d+)\r?\nTime taken: (\d+)/);
      let value523 ,timeTakenInMicroSeconds ;

      if(match){
        value = match[1];
        timeTakenInMicroSeconds = match[2];
      }
  
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


// ==== IGNORE THIS =====
// const inputData = ['1','2' , '26'];
//     const cppProcess = spawn(cppProgramPath, inputData);
//     let retVal ;
//     // Handle the standard output data
//     cppProcess.stdout.on('data', (data) => {
//       console.log(`Output from C++ program: ${data.toString()}`);
//       retVal = data.toString()
//       console.log(retVal)
//     });
    
//     // Handle errors
//     cppProcess.stderr.on('data', (data) => {
//       console.error(`Error from C++ program: ${data.toString()}`);
//     });
    
//     // Handle the end of the process
//     cppProcess.on('close', (code) => {
//       console.log(`C++ program exited with code ${code}`);
//       // res.json({ success: true, value: retVal });
//     });