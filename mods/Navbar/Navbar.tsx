import Link from 'next/link'
import { useState } from 'react'
import { useEffect } from 'react'
import Logo from './Logo'
import ProductsButton from './Products/Button'
import ProductsPage from './Products/Page'
import EmployeesButton from './Employees/Button'
import EmployeesPage from './Employees/Page'
import ContactButton from './Contact/Button'
import ApplyButton from './Apply/Button'
import DonateButton from './Donate/Button'

let Navbar = props => {
  let [isProductsOpen, setIsProductsOpen] = useState(false)
  let [isEmployeesOpen, setIsEmployeesOpen] = useState(false)
  let [isScroll, setIsScroll] = useState(false)

  let SetProducts = isActive => {
    setIsProductsOpen(isActive);
    setIsEmployeesOpen(false);
    setIsScroll(false);
  }

  let SetEmployees = isActive => {
    setIsProductsOpen(false);
    setIsEmployeesOpen(isActive);
    setIsScroll(false);
  }

  let SetScroll = isActive => {
    setIsEmployeesOpen(isEmployeesOpen)
    setIsProductsOpen(isProductsOpen)
    setIsScroll(isActive)
  }

  useEffect(() => {
    document.body.onscroll = () => {
      if (window.scrollY > 16) {
        SetScroll(true);
      } else {
        SetScroll(false);
      }
    }
  }, [])

  return <>
    <div className={`flex z-10 py-2 px-4 bg-white select-none shadow fixed items-center transition-all ${isScroll ? "top-4 right-4 left-4 rounded-xl": "top-0 left-0 right-0 rounded-none"}`} id="navbar">
      <div className="flex w-full">
        <Link href="/">
          <a><Logo/></a>
        </Link>
        <div className="pl-12 space-x-4 flex w-full">
          <ProductsButton bool={isProductsOpen} set={SetProducts}/>
          <EmployeesButton bool={isEmployeesOpen} set={SetEmployees}/>
          <DonateButton/>
          <ContactButton/>
        </div>
      </div>
      <div className="flex justify-end text-right w-full">
        <ApplyButton/>
      </div>
    </div>
    <ProductsPage bool={isProductsOpen}/>
    <EmployeesPage bool={isEmployeesOpen}/>
  </>
}

export default Navbar