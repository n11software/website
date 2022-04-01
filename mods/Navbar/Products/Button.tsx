import { ChevronDownIcon } from '@heroicons/react/solid'

let ProductsButton = props => {
  return <div className={`flex cursor-pointer space-x-2 items-center py-2 px-2 rounded-lg transition-colors ${props.bool ? "bg-black text-gray-200": "text-gray-800 hover:bg-stone-300"}`} onClick={()=>props.set(!props.bool)}>
    <span>Products</span>
    <ChevronDownIcon className={`h-4 w-4 transition-transform ease-in-out ${props.bool ? "rotate-180": ""}`} />
  </div>
}
export default ProductsButton