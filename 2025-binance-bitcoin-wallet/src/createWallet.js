// Importing dependencies
const bip32 = require('bip32');
const bip39 = require('bip39');
const bitcoin = require('bitcoinjs-lib');

// Defining the network (Bitcoin testnet in this case)
const network = bitcoin.networks.testnet;    // You can change this to bitcoin.networks.bitcoin for mainnet

const path = "m/49'/1'/0'/0/0"; // BIP44 path for testnet

// Creating mnemonic and seed
const mnemonic = bip39.generateMnemonic();
const seed = bip39.mnemonicToSeedSync(mnemonic);

// Creating root from seed
const root = bip32.fromSeed(seed, network);


// Deriving the node using the specified path
const child = root.derivePath(path);

// Getting the Bitcoin address
const { address } = bitcoin.payments.p2sh({
  redeem: bitcoin.payments.p2wpkh({ pubkey: child.publicKey, network }),
  network,
});

console.log("Mnemonic:", mnemonic);
console.log("Address:", address);
console.log("Private Key (WIF):", child.toWIF());