import React from "react";
import ReactDOM from "react-dom/client";
import { BrowserRouter, Routes, Route } from "react-router-dom";
import LoginPage from "../components/LoginPage";
import Dashboard from "../pages/Dashboard";
import AboutUs from "../pages/AboutUs";

const Routing = () => {
  return (
    <>
      <BrowserRouter>
        <Routes>
            <Route path="/" element={<LoginPage />}>
          </Route>
          <Route path="/dashboard" element={<Dashboard />}>
          </Route>
          <Route path="/aboutus" element={<AboutUs />}>
          </Route>
        </Routes>
      </BrowserRouter>
    </>
  );
};

export default Routing;
