const { exec } = require('child_process');
const workspaceRoot = process.cwd();
const file = process.argv[2];
exec(`/usr/bin/bash -c "${workspaceRoot}/scripts/ecompile ${file}"`, (error, stdout, stderr) => {
  if (error) {
    console.error(`exec error: ${error}`);
    return;
  }
  if (stdout) {
    console.log(`${stdout}`);
  }
  if (stderr) {
    console.error(`${stderr}`);
  }
});

