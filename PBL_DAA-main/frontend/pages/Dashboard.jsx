import {
  Box,
  Flex,
  HStack,
  Link,
  Text,
  Heading,
  Input,
  Button,
  VStack,
  Image,
} from "@chakra-ui/react";
import { useState } from "react";
import Navbar from "../components/Navbar";
import LogSection from "../components/LogSection";
import TabSection from "../components/TabSection";

const Dashboard = () => {
  const [imageURL] = useState("https://cdn2.iconfinder.com/data/icons/seo-web/512/website-code-1024.png"); // Replace this URL

  return (
    <Box
      minH="100vh"
      bgGradient="linear(to-r, #0f2027, #203a43, #2c5364)"
      px={[4, 8, 20]}
      py={8}
      color="white"
    >
      {/* Navbar */}
      <Navbar />

      {/* Main content */}
      <LogSection />

    </Box>
  );
};

export default Dashboard;
