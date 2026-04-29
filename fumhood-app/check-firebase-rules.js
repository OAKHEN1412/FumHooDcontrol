#!/usr/bin/env node

/**
 * Firebase Rules Status Checker for fumhood-ac
 * Verifies if the updated database rules are deployed
 * 
 * Usage: node check-firebase-rules.js
 */

const https = require('https');

const FIREBASE_URL = 'https://fumhood-ac-default-rtdb.asia-southeast1.firebasedatabase.app/.settings/rules.json';
const PROJECT_ID = 'fumhood-ac';

console.log('🔍 Checking Firebase Rules Status for fumhood-ac\n');

https.get(FIREBASE_URL, (res) => {
  let data = '';
  
  res.on('data', (chunk) => {
    data += chunk;
  });
  
  res.on('end', () => {
    try {
      const rules = JSON.parse(data);
      
      console.log('✅ Rules retrieved successfully\n');
      console.log('📋 Current Rules Structure:');
      console.log(JSON.stringify(rules, null, 2));
      
      // Check for new rules
      if (rules.rules && rules.rules.users) {
        const usersRead = rules.rules.users['.read'];
        
        if (usersRead === 'auth != null') {
          console.log('\n✅ NEW RULES DEPLOYED!');
          console.log('   Users can now read/write their own profiles');
          console.log('   Devices can be paired by authenticated users');
          console.log('\n🎯 Ready for testing:');
          console.log('   1. Sign up new user');
          console.log('   2. Login');
          console.log('   3. Add device');
          process.exit(0);
        } else if (usersRead && usersRead.includes('admins')) {
          console.log('\n⚠️  OLD RULES STILL ACTIVE');
          console.log('   Users must be admins to access data');
          console.log('\n📝 ACTION NEEDED:');
          console.log('   Deploy new rules via Firebase Console');
          console.log('   See: RULES_DEPLOYMENT_GUIDE.md');
          process.exit(1);
        }
      }
      
      console.log('\n❓ Unknown rules structure');
      process.exit(1);
      
    } catch (err) {
      console.error('❌ Error parsing rules:', err.message);
      process.exit(1);
    }
  });
  
}).on('error', (err) => {
  console.error('❌ Firebase connection error:', err.message);
  console.log('\n💡 Make sure:');
  console.log('   - Network connection is active');
  console.log('   - Project fumhood-ac is accessible');
  process.exit(1);
});
