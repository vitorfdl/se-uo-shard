const { spawn } = require('child_process');

const subprocess = spawn('./pol', []);
subprocess.on('close', (code, signal) => {
   console.log(`child process terminated due to receipt of signal ${signal}`);

   process.exit();
 });