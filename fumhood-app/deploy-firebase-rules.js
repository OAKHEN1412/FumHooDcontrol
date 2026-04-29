#!/usr/bin/env node

/**
 * FumHood Firebase Rules Auto-Deployer
 * Deploys updated Realtime Database rules using Firebase CLI
 * 
 * Usage: node deploy-firebase-rules.js
 */

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const PROJECT_ID = 'fumhood-ac';
const RULES_FILE = './database.rules.json';

console.log('🔥 FumHood Firebase Rules Deployer\n');

// Check if rules file exists
if (!fs.existsSync(RULES_FILE)) {
  console.error(`❌ Error: ${RULES_FILE} not found`);
  process.exit(1);
}

// Read rules
const rulesContent = fs.readFileSync(RULES_FILE, 'utf-8');
console.log(`✓ Rules file loaded: ${RULES_FILE}`);
console.log(`✓ File size: ${(fs.statSync(RULES_FILE).size / 1024).toFixed(2)} KB\n`);

// Check if firebase CLI is installed
try {
  execSync('firebase --version', { stdio: 'pipe' });
} catch {
  console.log('⚠️  Firebase CLI not found. Installing globally...\n');
  try {
    execSync('npm install -g firebase-tools', { stdio: 'inherit' });
    console.log('\n✓ Firebase CLI installed\n');
  } catch {
    console.error('❌ Failed to install Firebase CLI');
    process.exit(1);
  }
}

// Validate JSON
try {
  JSON.parse(rulesContent);
  console.log('✓ Rules JSON is valid\n');
} catch (err) {
  console.error(`❌ Invalid JSON in rules file: ${err.message}`);
  process.exit(1);
}

// Deploy
console.log(`📦 Deploying rules to project: ${PROJECT_ID}\n`);
console.log('Instructions:');
console.log('1. A browser will open - login with your Google account');
console.log('2. Grant access to Firebase CLI');
console.log('3. Rules will deploy automatically\n');

try {
  execSync(`firebase use ${PROJECT_ID}`, { stdio: 'inherit' });
  console.log('\n');
  execSync('firebase deploy --only database:rules', { stdio: 'inherit' });
  console.log('\n✅ Rules deployed successfully!');
  console.log('\n📋 Next steps:');
  console.log('1. Start the app: npm start');
  console.log('2. Create new account: test@fumhood.local / test123');
  console.log('3. Try pairing device: 9C139E or BBC9E4');
} catch (err) {
  console.error(`\n❌ Deployment failed: ${err.message}`);
  console.log('\n💡 Troubleshooting:');
  console.log('1. Make sure you\'re logged in: firebase login');
  console.log('2. Check internet connection');
  console.log('3. Verify project ID: fumhood-ac');
  process.exit(1);
}
