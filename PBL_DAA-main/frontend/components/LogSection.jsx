import { useState } from "react";
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
import TabSection from "./TabSection";

const LogSection = () => {
  const [imageURL] = useState(
    "https://cdn2.iconfinder.com/data/icons/seo-web/512/website-code-1024.png"
  ); // Replace this URL

  return (
    <Flex
      direction={["column", "column", "row"]}
      alignItems="center"
      justifyContent="space-between"
    >
      {/* Left section */}
      <Box maxW="lg">
        <Heading fontSize="3xl" mb={4}>
          Track every commit, branch, and snapshot
        </Heading>
        <TabSection />
      </Box>

      {/* Right section */}
      <Box
        position="relative"
        mt={[12, 12, 0]}
        ml={[0, 0, 12]}
        borderRadius="full"
        overflow="hidden"
        boxShadow="2xl"
        w={["100%", "80%", "350px"]}
        h="350px"
      >
        <Image
          src={imageURL}
          alt="Newsletter Visual"
          w="100%"
          h="100%"
          objectFit="cover"
        />
      </Box>
    </Flex>
  );
};

export default LogSection;
