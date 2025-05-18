import {
  Box,
  Flex,
  Heading,
  Text,
  Avatar,
  VStack,
  SimpleGrid,
  Button,
} from "@chakra-ui/react";
import Navbar from "../components/Navbar";
import { useNavigate } from "react-router-dom";

const team = [
  { name: "Akhilesh Padiyar" },
  { name: "Ayush Gupta" },
  { name: "Sujal Verma" },
  { name: "Divyanshu Tomar" },
];

const AboutUs = () => {
  const navigate = useNavigate();
  return (
    <Box
      minH="100vh"
      bgGradient="linear(to-r, #0f2027, #203a43, #2c5364)"
      px={[4, 8, 20]}
      py={[12, 16]}
      color="white"
    >
      {/* Page Heading */}
      <Heading fontSize="4xl" mb={4}>
        About Us
      </Heading>
      <Text fontSize="lg" color="gray.300" mb={12} maxW="3xl">
        We are a passionate team of developers building a version control system
        for the future. Meet our team members below.
      </Text>

      {/* Team Grid */}
      <SimpleGrid columns={[1, 2, 4]} spacing={10}>
        {team.map((member, idx) => (
          <VStack
            key={idx}
            bg="rgba(255,255,255,0.05)"
            p={6}
            borderRadius="lg"
            spacing={4}
            textAlign="center"
            boxShadow="md"
            _hover={{ transform: "scale(1.05)", transition: "0.3s" }}
          >
            <Avatar name={member.name} size="xl" bg="blue.400" color="white" />
            <Text fontSize="lg" fontWeight="bold">
              {member.name}
            </Text>
          </VStack>
        ))}
      </SimpleGrid>
      <Button
      mt={"25px"}
        _hover={{ bg: "black", color : "white"}}
        onClick={(e) => {
          e.preventDefault();
          navigate("/dashboard");
        }}
      >
        Back to dashboard
      </Button>
    </Box>
  );
};

export default AboutUs;
