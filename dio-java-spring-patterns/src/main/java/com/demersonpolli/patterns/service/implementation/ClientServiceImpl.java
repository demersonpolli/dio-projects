package com.demersonpolli.patterns.service.implementation;

import java.util.Optional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.demersonpolli.patterns.model.Client;
import com.demersonpolli.patterns.model.ClientRepository;
import com.demersonpolli.patterns.model.Address;
import com.demersonpolli.patterns.model.AddressRepository;
import com.demersonpolli.patterns.service.ClientService;
import com.demersonpolli.patterns.service.ViaCepService;

@Service
public class ClientServiceImpl implements ClientService {
    // Singleton: Inject the Spring components with @Autowired.
    @Autowired
    private ClientRepository clientRepository;
    @Autowired
    private AddressRepository addressRepository;
    @Autowired
    private ViaCepService viaCepService;

    // Strategy: implement all interface methods
    // Facade: Abstract integration with subsystems, keeping the interface simple.

    @Override
    public Iterable<Client> findAll() {
        return clientRepository.findAll();
    }

    @Override
    public Client findById(Long id) {
        Optional<Client> client = clientRepository.findById(id);
        return client.orElseThrow();
    }

    @Override
    public void insert(Client client) {
        saveClientWithCep(client);
    }

    @Override
    public void update(Long id, Client client) {
        Optional<Client> clientDb = clientRepository.findById(id);
        if (clientDb.isPresent()) {
            saveClientWithCep(client);
        }
    }

    @Override
    public void delete(Long id) {
        clientRepository.deleteById(id);
    }

    private void saveClientWithCep(Client client) {
        // Verify if the client's address already exists (by the ZIP code).
        String cep = client.getAddress().getCep();
        Address address = addressRepository.findById(cep).orElseGet(() -> {
            // If it doesn't exist, get address from ViaCEP.
            Address newAddress = viaCepService.searchCep(cep);
            addressRepository.save(newAddress);
            return newAddress;
        });
        client.setAddress(address);
        clientRepository.save(client);
    }
}
