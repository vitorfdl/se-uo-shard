const { spawn } = require('child_process');

const subprocess = spawn('./pol', [], {
   stdio: 'inherit',
   shell: true
 });

function ServiceExited(code, signal) {
   console.log(`child process terminated due to receipt of signal ${signal}`);
   process.exit();
}

subprocess.on('close', ServiceExited);
subprocess.on('exit', ServiceExited);

// subprocess.stdout.on('data', (data) => {
//    console.log(`${data}`);
//  });
 
//  subprocess.stderr.on('data', (data) => {
//    console.error(`${data}`);
//  });