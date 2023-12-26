const { spawn } = require('child_process');
const express = require('express');
const bodyParser = require('body-parser');

const app = express();
const port = 3000; // You can use any port you prefer

// Use bodyParser middleware to parse JSON requests
app.use(bodyParser.json());

// Path to your C++ program
const cppProgramPath = './harmonia_sequential_inserts.exe';

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

  if (key && value) {

    const cppProcess = spawn(cppProgramPath, inputData);

    // Handle the standard output data
    cppProcess.stdout.on('data', (data) => {
      console.log(`Output from C++ program: ${data.toString()}`);
    });
    
    // Handle errors
    cppProcess.stderr.on('data', (data) => {
      console.error(`Error from C++ program: ${data.toString()}`);
    });
    
    // Handle the end of the process
    cppProcess.on('close', (code) => {
      console.log(`C++ program exited with code ${code}`);
    });

      res.json({ success: true, message: 'Data inserted successfully' });
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
      retVal = retVal.trim();
      res.json({ success: true, value: retVal });
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