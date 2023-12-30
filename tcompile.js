const { exec } = require('child_process');
const workspaceRoot = process.cwd();
let file = process.argv[2];

file = file.replace(workspaceRoot, '.');

file = file.replace('include/', '');



exec(`"./scripts/ecompile" -u ${file}`, (error, stdout, stderr) => {
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

