package com.example.projek4.controller;

import com.example.projek4.entity.Ticket;
import com.example.projek4.service.TicketService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

@Controller
public class ParkingController {

    @Autowired
    private TicketService ticketService;

    @GetMapping("/checkin")
    public String checkInForm(Model model) {
        model.addAttribute("ticket", new Ticket());
        return "checkin";
    }

    @PostMapping("/checkin")
    public String checkInSubmit(@ModelAttribute Ticket ticket, Model model) {
        Ticket result = ticketService.checkIn(ticket.getPlateNumber());

        if (result == null) {
            model.addAttribute("errorMessage", "Vehicle with the same plate number is already in the parking area.");
            return "checkin";
        }

        model.addAttribute("ticket", result);
        return "checkinresult";
    }

    @GetMapping("/checkout")
    public String checkOutForm(Model model) {
        model.addAttribute("ticket", new Ticket());
        return "checkout";
    }

    @PostMapping("/checkout")
    public String checkOutSubmit(@ModelAttribute Ticket ticket, Model model) {
        Ticket result = ticketService.checkOut(ticket.getPlateNumber());

        if (result == null) {
            model.addAttribute("errorMessage", "Vehicle with the given plate number is not in the parking area.");
            return "checkout";
        }

        model.addAttribute("ticket", result);
        return "checkoutresult";
    }
}
