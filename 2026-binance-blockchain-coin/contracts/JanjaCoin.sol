// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/token/ERC20/extensions/ERC20Permit.sol";

contract Janja is ERC20, ERC20Permit {
    mapping (address => uint256) balances;
    mapping (address => mapping(address => uint256)) allowed;

    uint _totalSupply = 0.0001 ether;

    constructor() ERC20("Janja", "JPT") ERC20Permit("Janja") {
        // At contract creation all money is owned by me.
        balances[msg.sender] = _totalSupply;
    }

    function balanceOf(address tokenOwner) public override view returns (uint256) {
        return balances[tokenOwner];
    }

    function transfer(address receiver, uint256 tokens) public override returns (bool) {
        require(tokens <= balances[msg.sender]);
        balances[msg.sender] = balances[msg.sender] - tokens;
        balances[receiver] = balances[receiver] + tokens;
        emit Transfer(msg.sender, receiver, tokens);
        return true;
    }

    function approve(address delegate, uint256 tokens) public override returns (bool) {
        allowed[msg.sender][delegate] = tokens;
        emit Approval(msg.sender, delegate, tokens);
        return true;
    }

    function allowance(address owner, address delegate) public override view returns (uint) {
        return allowed[owner][delegate];
    }

    function transferFrom(address owner, address buyer, uint256 tokens) public override returns (bool) {
        require(tokens <= balances[owner]);
        require(tokens <= allowed[owner][msg.sender]); // 1372400

        balances[owner] = balances[owner] - tokens;
        allowed[owner][msg.sender] = allowed[owner][msg.sender] - tokens;
        balances[buyer] = balances[buyer] + tokens;
        emit Transfer(owner, buyer, tokens);
        return true;
    }
}
