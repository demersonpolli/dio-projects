package com.demersonpolli.patterns.service;

import com.demersonpolli.patterns.model.Client;

/**
 * This interface defines the default <b>Strategy</b> for the client side.
 * With this, if needed, we can define multiple implementations.
 */
public interface ClientService {
    Iterable<Client> findAll();
    Client findById(Long id);
    void insert(Client client);
    void update(Long id, Client client);
    void delete(Long id);
}
