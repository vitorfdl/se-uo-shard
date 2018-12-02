const { spawn } = require('child_process');

console.log('starting process');
let subprocess = spawn('./pol', [], {
   stdio: 'inherit',
   shell: true
 });

async function ServiceExited(code, signal) {
   console.log(`>>>>>>>>>>>>>>>>>>>>>>>>> child process terminated due to receipt of signal ${signal} ${code}`);
   if (signal) return process.exit();

   subprocess = spawn('./pol', [], {
      stdio: 'inherit',
      shell: true
   });
}

subprocess.on('close', ServiceExited);
subprocess.on('exit', ServiceExited);
subprocess.on('SIGINT', () => {
   process.exit();
});

process.on('SIGINT', function() {
   console.log("Caught interrupt signal");

   subprocess.kill('SIGINT');
});

// subprocess.stdout.on('data', (data) => {
//    console.log(`${data}`);
//  });
 
//  subprocess.stderr.on('data', (data) => {
//    console.error(`${data}`);
//  });