import React, { useState } from "react";
import {
  Box,
  Button,
  Flex,
  FormControl,
  FormLabel,
  Heading,
  Input,
  useToast,
} from "@chakra-ui/react";
import { Link, Outlet, useNavigate } from "react-router-dom";


export default function LoginPage() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const toast = useToast();
  const navigate = useNavigate();

  const handleSubmit = (e) => {
    e.preventDefault();
    console.log("Login submitted:", { email, password });

    // Example toast notification
    toast({
      title: "Logged in.",
      description: "You have successfully logged in.",
      status: "success",
      duration: 3000,
      isClosable: true,
    });

    navigate('/dashboard')

    // Add actual login logic here (e.g. API call)
  };

  return (
    <Flex minH="100vh" align="center" justify="center" bg="gray.100">
      <Box
        bg="white"
        p={8}
        rounded="xl"
        shadow="lg"
        w={{ base: "90%", sm: "400px" }}
      >
        <Heading mb={6} textAlign="center">
          Login
        </Heading>
        <form onSubmit={handleSubmit}>
          <FormControl id="email" mb={4} isRequired>
            <FormLabel>Email</FormLabel>
            <Input
              type="email"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              placeholder="Enter your email"
            />
          </FormControl>
          <FormControl id="password" mb={6} isRequired>
            <FormLabel>Password</FormLabel>
            <Input
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              placeholder="Enter your password"
            />
          </FormControl>
          <Button type="submit" colorScheme="blue" width="full">
            Sign In
          </Button>
        </form>
        <Outlet />
      </Box>
    </Flex>
  );
}
