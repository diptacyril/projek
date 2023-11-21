package com.example.projek4.service;

import com.example.projek4.entity.Ticket;

import java.util.List;

public interface TicketService {

    List<Ticket> getAllTickets();

    Ticket getTicketById(Long id);

    Ticket saveTicket(Ticket ticket);

    void deleteTicket(Long id);

    // Add the following method declaration
    void checkIn(String ticketNumber);
}
