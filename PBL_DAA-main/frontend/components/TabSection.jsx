import {
  Tabs,
  TabList,
  TabPanels,
  Tab,
  TabPanel,
  Input,
  Button,
  useToast,
} from "@chakra-ui/react";
import React, { useState } from "react";
import {
  FormControl,
  FormLabel,
  FormErrorMessage,
  FormHelperText,
} from "@chakra-ui/react";

const TabSection = () => {
    const [input, setInput] = useState();
    const toast = useToast();
  const handleSubmit = (e) => {
    e.preventDefault();
    setInput("")
    toast({
      title: "Created",
      description: "Repository intialized successfully.",
      status: "success",
      duration: 3000,
      isClosable: true,
    });
  };
  return (
    <>
      <Tabs variant="soft-rounded" colorScheme="green">
        <TabList>
          <Tab color={"white"}>Initialize repository</Tab>
          <Tab color={"white"}>Commit Changes</Tab>
          <Tab color={"white"}>Check commit History</Tab>
        </TabList>
        <TabPanels>
          <TabPanel>
            <form onSubmit={handleSubmit}>
              <FormControl id="email" mb={4} isRequired>
                <FormLabel>Repository name</FormLabel>
                <Input
                  type="text"
                  placeholder="Enter repository name"
                  value={input}
                  onChange={(e) => setInput(e.target.value)}
                />
              </FormControl>
              <Button type="submit" colorScheme="blue" width="full">
                Create
              </Button>
            </form>
          </TabPanel>
          <TabPanel>
            <p>two!</p>
          </TabPanel>
          <TabPanel>
            <p>one!</p>
          </TabPanel>
        </TabPanels>
      </Tabs>
    </>
  );
};

export default TabSection;
